#pragma once

#include "carrier_file.h"
#include "utils/stego_types.h"

#include <tuple>

extern "C"
{
	#include <libavformat/avformat.h>
}

namespace stego_disk
{
	/**
	 * Forward declarations
	 */
	class MemoryBuffer;
	class ContainerHandler;

	class CarrierFileMPEG : public CarrierFile {
	public:
		CarrierFileMPEG(File file, std::shared_ptr<Encoder> encoder, std::shared_ptr<Permutation> permutation, std::unique_ptr<Fitness> fitness);

		virtual void LoadFile() override;
		virtual void SaveFile() override;
	private:
		uint64 CalculateCapacity() const;
		void LoadBuffer(MemoryBuffer &buffer);
		void SaveBuffer(const MemoryBuffer &buffer);
		bool ReadByte(const AVPacket &packet, MemoryBuffer &buffer, std::size_t &offset);
		bool WriteByte(AVPacket &packet, const MemoryBuffer &buffer, std::size_t &offset, bool update_dts);
		uint64 ModifyLSB(uint64 value, uint64 lsb) const;
		std::tuple<uint64, uint64, uint64> GetTestValues(const AVPacket &packet) const;
	private:
		std::unique_ptr<ContainerHandler> container_handler_{ nullptr };
	};
}