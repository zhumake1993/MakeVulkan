#include "Tools.h"
#include "Log.h"
#include <fstream>

#include "Platforms.h"
#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include <android/asset_manager.h>
#endif
std::vector<char> GetBinaryFileContents(std::string const &filename) {

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	AAsset* asset = AAssetManager_open(platform::GetAndroidApp()->activity->assetManager, filename.c_str(), AASSET_MODE_STREAMING);
	if (!asset)
	{
		LOG("Could not open file(%s)\n", filename.c_str());
		return std::vector<char>();
	}
	size_t size = AAsset_getLength(asset);

	std::vector<char> data(size);
	AAsset_read(asset, data.data(), size);
	AAsset_close(asset);
#else
	std::ifstream file(filename, std::ios::binary);
	if (file.fail())
	{
		LOG("Could not open file(%s)\n", filename.c_str());
		return std::vector<char>();
	}

	std::streampos begin, end;
	begin = file.tellg();
	file.seekg(0, std::ios::end);
	end = file.tellg();

	std::vector<char> data(static_cast<size_t>(end - begin));
	file.seekg(0, std::ios::beg);
	file.read(&data[0], end - begin);
	file.close();
#endif

	if (data.size() == 0)
	{
		LOG("The size of file(%s) is 0\n", filename.c_str());
	}

	return data;
}

void * AlignedAlloc(size_t size, size_t alignment)
{
	void *data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
	data = _aligned_malloc(size, alignment);
#else
	int res = posix_memalign(&data, alignment, size);
	if (res != 0)
		data = nullptr;
#endif
	return data;
}

void AlignedFree(void * data)
{
#if	defined(_MSC_VER) || defined(__MINGW32__)
	_aligned_free(data);
#else
	free(data);
#endif
}
