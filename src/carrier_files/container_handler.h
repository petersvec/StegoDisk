#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "utils/non_copyable.h"

extern "C"
{
	#include <libavformat/avformat.h>
}

namespace stego_disk
{
	class ContainerHandler;

	enum class StreamType : unsigned 
	{
		Unknown		= 0,
		Video		= 1,
		Audio		= 2,
		Subtitles	= 3
	};

	using PacketUPtr = std::unique_ptr<AVPacket>;
	using StreamData = std::map<StreamType, std::vector<std::reference_wrapper<AVPacket>>>;
	using ContainerHandlerUPtr = std::unique_ptr<ContainerHandler>;

	class ContainerHandler : public NonCopyable {
	public:
		explicit ContainerHandler(std::string_view name);
		ContainerHandler();
		~ContainerHandler();

		ContainerHandler(const ContainerHandler &&) = delete;
		ContainerHandler& operator=(const ContainerHandler &&) = delete;

		void SetFileName(std::string_view name);
		std::string GetFileName() const;

		void Init();
		void Load();
		void Save();

		bool HasBFrames() const;

		StreamData& GetStreamData();
		std::vector<PacketUPtr>& GetData();
	private:
		void Close();
		StreamType GetStreamType(int type) const;
		std::string GetStreamStr(StreamType type) const;
		void SetStreamParameters(AVStream *input, AVStream *output) const;
		bool BFrameDetection();
	private:
		std::string file_name_{ "" };
		AVFormatContext *input_context_{ nullptr };
		AVFormatContext *output_context_{ nullptr };
		std::vector<PacketUPtr> data_;
		StreamData stream_data_;
		bool has_b_frames_{ false };
	};
}