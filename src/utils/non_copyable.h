#pragma  once

namespace stego_disk
{
	/**
	 * Inherit from this class if you want to make derived class non copyable
	 */
	class NonCopyable {
	protected:
		NonCopyable() = default;
		~NonCopyable() = default;
	private:
		NonCopyable(const NonCopyable&);
		NonCopyable& operator=(const NonCopyable&);
	};
}