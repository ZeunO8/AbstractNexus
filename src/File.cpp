#include <anex/File.hpp>
#include <sstream>
#include <iomanip>
#include <chrono>
using namespace anex;
File::File(const std::string& filePath, enums::EFileLocation fileLocation, const std::string& mode)
    : originalFilePath(filePath), filePath(filePath), fileLocation(fileLocation), openMode(std::ios::in | std::ios::out | std::ios::binary)
{
    if (mode.find('r') != std::string::npos)
    {
        openMode |= std::ios::in;
    }
    if (mode.find('w') != std::string::npos)
    {
        openMode |= std::ios::out;
    }
    if (mode.find('a') != std::string::npos)
    {
        openMode |= std::ios::app;
    }
    if (mode.find('+') != std::string::npos)
    {
        openMode |= std::ios::in | std::ios::out;
    }
    open();
};
File& File::operator=(const File& other)
{
    if (this != &other)
    {
        originalFilePath = other.originalFilePath;
        filePath = other.filePath;
        fileLocation = other.fileLocation;
        openMode = other.openMode;
        if (fileStream.is_open())
        {
            fileStream.close();
        }
    }
    return *this;
};
File::~File()
{
    close();
};
bool File::open()
{
    fileStream.open(filePath, openMode);
    return fileStream.is_open();
};
bool File::close()
{
    if (fileStream.is_open())
  {
        fileStream.close();
        return true;
    }
    return false;
};
bool File::readBytes(size_t index, size_t sizeBytes, void* pointer)
{
    if (!fileStream.is_open())
    {
        return false;
    }
    fileStream.seekg(index, std::ios::beg);
    fileStream.read(static_cast<char*>(pointer), sizeBytes);
    return fileStream.good();
};
bool File::writeBytes(size_t index, size_t sizeBytes, const void* pointer)
{
    if (!fileStream.is_open())
  {
        return false;
    }
    fileStream.seekp(index, std::ios::beg);
    fileStream.write(static_cast<const char*>(pointer), sizeBytes);
    return fileStream.good();
};
bool File::truncate(size_t newFileSize)
{
    if (!fileStream.is_open())
    {
        return false;
    }
    fileStream.close();
    std::filesystem::resize_file(filePath, newFileSize);
    return open();
};
bool File::sync()
{
    if (fileStream.is_open())
    {
        fileStream.flush();
        return true;
    }
    return false;
};
size_t File::size()
{
    try
    {
        return std::filesystem::file_size(filePath);
    }
    catch (const std::filesystem::filesystem_error&)
    {
        return 0;
    }
};
bool File::exists() const
{
    return std::filesystem::exists(filePath);
};
time_t File::lastModified() const
{
    auto ftime = std::filesystem::last_write_time(filePath);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sctp);
};
bool File::remove()
{
    if (std::filesystem::exists(filePath))
  {
        std::filesystem::remove(filePath);
        return true;
    }
    return false;
};
bool File::exists(const std::string& path)
{
    return std::filesystem::exists(path);
};
std::string File::getUserDirectoryPath()
{
    return std::filesystem::path(getenv("HOME")).string();
};
std::string File::getProgramDirectoryPath()
{
    return std::filesystem::current_path().string();
};
std::string File::getProgramDataPath()
{
    return std::filesystem::temp_directory_path().string();
};
std::string File::getExecutableName()
{
    return std::filesystem::path(getenv("_"))
        .filename()
        .string();
};
std::string File::toPlatformPath(const std::string& path)
{
    std::string platformPath = path;
    std::replace(platformPath.begin(), platformPath.end(), '\\', '/');
    return platformPath;
};
std::string File::toString()
{
    auto actualSize = size();
    std::string string;
    string.resize(actualSize);
    readBytes(0, actualSize, string.data());
    return string;
};
std::shared_ptr<int8_t[]> File::toBytes()
{
    auto actualSize = size();
    std::shared_ptr<int8_t[]> bytes = std::shared_ptr<int8_t[]>(new int8_t[actualSize]);
    readBytes(0, actualSize, bytes.get());
    return bytes;
};