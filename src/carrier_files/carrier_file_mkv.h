#pragma once

#include "carrier_file.h"
#include "utils/stego_types.h"

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

	class CarrierFileMKV : public CarrierFile {
	public:
		CarrierFileMKV(File file, std::shared_ptr<Encoder> encoder, std::shared_ptr<Permutation> permutation, std::unique_ptr<Fitness> fitness);

		virtual void LoadFile() override;
		virtual void SaveFile() override;
	private:
		uint64 CalculateCapacity() const;
		void LoadBuffer(MemoryBuffer &buffer);
		void SaveBuffer(const MemoryBuffer &buffer);
	private:
		std::unique_ptr<ContainerHandler> container_handler_{ nullptr };
	};
}