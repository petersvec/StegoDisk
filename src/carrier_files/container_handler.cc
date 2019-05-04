#include "container_handler.h"
#include "utils/exceptions.h"
#include "logging/logger.h"
#include "utils/stego_config.h"

#include <fstream>

namespace stego_disk
{
	ContainerHandler::ContainerHandler(std::string_view name)
		:file_name_(name)
	{
		Init();
	}

	ContainerHandler::ContainerHandler()
	{

	}

	ContainerHandler::~ContainerHandler()
	{

	}

	void ContainerHandler::SetFileName(std::string_view name)
	{
		file_name_ = name;
	}

	std::string ContainerHandler::GetFileName() const
	{
		return file_name_;
	}

	void ContainerHandler::Load()
	{
		LOG_INFO("Loading container: " + file_name_);

		auto ret = this->BFrameDetection();

		while (true)
		{
			auto packet = std::make_unique<AVPacket>();
			AVStream *stream{ nullptr };

			if (av_read_frame(input_context_, packet.get()) < 0)
			{
				break;
			}

			stream = input_context_->streams[packet->stream_index];
			auto stream_type = GetStreamType(stream->codecpar->codec_type);

			LOG_TRACE("Packet read, pts: " + std::to_string(packet->pts) +
				" dts: " + std::to_string(packet->dts) +
				" stream: " + this->GetStreamStr(stream_type));
			
			// check if video codec uses B frames (WIP, probably needs more testing...) 
			// if codec uses only I and P frames, both pts and dts should be the same
			// in same cases difference could be 1, even without B frames
			// therefore if difference between pts and dts is 2 and larger,
			// we can guess that codec uses B frames
			// !!! only if we have no ffprobe specified in configuration file or ffprobe failed
			if ((StegoConfig::ffprobe_path().empty() || !ret) && stream_type == StreamType::Video && std::abs(packet->pts - packet->dts) >= 2)
			{
				has_b_frames_ = true;
			}

			data_.emplace_back(std::move(packet));
			stream_data_[stream_type].emplace_back(std::ref(*data_.back()));
		}

		LOG_DEBUG("B frames detected: " + std::to_string(this->has_b_frames_));
	}

	void ContainerHandler::Save()
	{
		LOG_INFO("Saving container: " + file_name_);

		if (avformat_alloc_output_context2(&output_context_, nullptr, nullptr, file_name_.c_str()) < 0)
		{
			LOG_ERROR("Failed to allocate output context: " + file_name_);
			throw exception::AllocError();
		}

		for (auto i = 0u; i < input_context_->nb_streams; i++)
		{
			AVStream *input_stream = input_context_->streams[i];
			AVStream *output_stream = avformat_new_stream(output_context_, nullptr);

			if (!output_context_)
			{
				throw exception::AllocError();
			}

			if (avcodec_parameters_copy(output_stream->codecpar, input_stream->codecpar) < 0)
			{
				LOG_ERROR("Failed to copy stream parameters: " + file_name_);
				throw exception::IoError{ file_name_ };
			}

			this->SetStreamParameters(input_stream, output_stream);
		}

		if (avio_open(&output_context_->pb, file_name_.c_str(), AVIO_FLAG_WRITE) < 0)
		{
			LOG_ERROR("Failed to open output file: " + file_name_);
			throw exception::IoError{ file_name_ };
		}

		if (avformat_write_header(output_context_, nullptr) < 0)
		{
			LOG_ERROR("Failed to write container header: " + file_name_);
			throw exception::IoError{ file_name_ };
		}

		for (auto &packet : data_)
		{
			if (av_write_frame(output_context_, packet.get()) < 0)
			{
				LOG_ERROR("Failed to write packet to output file: " + file_name_);
				LOG_TRACE("Packet write, pts: " + std::to_string(packet->pts) +
					" dts: " + std::to_string(packet->dts) +
					" stream: " + this->GetStreamStr(GetStreamType(packet->stream_index)));
				throw exception::IoError{ file_name_ };
			}

			av_packet_unref(packet.get());
		}

		av_write_trailer(output_context_);
		this->Close();
	}

	bool ContainerHandler::HasBFrames() const
	{
		return has_b_frames_;
	}

	stego_disk::StreamData& ContainerHandler::GetStreamData()
	{
		return stream_data_;
	}

	std::vector<stego_disk::PacketUPtr>& ContainerHandler::GetData()
	{
		return data_;
	}

	void ContainerHandler::Init()
	{
		LOG_INFO("Initializing container handler: " + file_name_);

		if(avformat_open_input(&input_context_, file_name_.c_str(), nullptr, nullptr) < 0)
		{
			LOG_ERROR("Failed to open: " + file_name_);
			throw exception::IoError{ file_name_ };
		}

		if (avformat_find_stream_info(input_context_, nullptr) < 0)
		{
			LOG_ERROR("Failed to find stream info: " + file_name_);
			throw exception::ParseError{ file_name_, "Could not find stream info" };
		}
	}

	void ContainerHandler::Close()
	{
		if (input_context_)
		{
			LOG_DEBUG("Closing input context: " + file_name_);
			avformat_close_input(&input_context_);
		}

		if (output_context_ && !(output_context_->flags & AVFMT_NOFILE))
		{
			LOG_DEBUG("Closing output context: " + file_name_);
			avio_closep(&output_context_->pb);
			avformat_free_context(output_context_);
		}
	}

	StreamType ContainerHandler::GetStreamType(int type) const
	{
		switch (type)
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				return StreamType::Video;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				return StreamType::Audio;
			}
			case AVMEDIA_TYPE_SUBTITLE:
			{
				return StreamType::Subtitles;
			}
			default:
			{
				return StreamType::Unknown;
			}
		}
	}

	std::string ContainerHandler::GetStreamStr(StreamType type) const
	{
		switch (type)
		{
			case StreamType::Video:
			{
				return "video";
			}
			case StreamType::Audio:
			{
				return "audio";
			}
			case StreamType::Subtitles:
			{
				return "subtitles";
			}
			default:
			{
				return "unknown";
			}
		}
	}

	void ContainerHandler::SetStreamParameters(AVStream *input, AVStream *output) const
	{
		output->codecpar->codec_tag = 0;
		output->time_base = input->time_base;
		output->avg_frame_rate = input->avg_frame_rate;
		output->duration = input->duration;
		output->first_dts = input->first_dts;
		output->nb_frames = input->nb_frames;
		output->r_frame_rate = input->r_frame_rate;
		output->start_time = input->start_time;
	}

	bool ContainerHandler::BFrameDetection()
	{
		if (!StegoConfig::ffprobe_path().empty())
		{
			auto command = StegoConfig::ffprobe_path() + " -show_frames -read_intervals \"\%+#30\" " + file_name_ + " > out.txt";
			
			std::ofstream tmp("out.txt");
			tmp.close();

			if (auto ret = system(command.c_str()); ret == 0)
			{
				std::ifstream input("out.txt");
				std::string result((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
				input.close();

				if (result.find("pict_type=B", 0) != std::string::npos)
				{
					has_b_frames_ = true;
				}
				else
				{
					has_b_frames_ = false;
				}

				return true;
			}
		}

		return false;
	}
}