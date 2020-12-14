#include <PDPC/Common/File.h>
#include <PDPC/Common/Log.h>
#include <PDPC/Common/Assert.h>

using namespace pdpc;

void test(int n, const std::string& path, const std::string& true_filename, const std::string& true_ext)
{
    const auto ext      = get_extension(path);
    const auto filename = get_filename(path);

    info() << n << ". [" << path << "] -> [" << filename << "].[" << ext << "]";

    bool has_error = false;

    if(true_ext      != ext     ) {error() << "ext:  " << ext      << "!=" << true_ext      << " (expected)"; has_error = true;}
    if(true_filename != filename) {error() << "name: " << filename << "!=" << true_filename << " (expected)"; has_error = true;}

    PDPC_ASSERT(has_error == false);
}

int main()
{
    test(0, "",                       "",         "");
    test(1, "filename",               "filename", "");
    test(2, "./filename",             "filename", "");
    test(3, "filename.ext",           "filename", "ext");
    test(4, "./filename.ext",         "filename", "ext");
    test(5, "/filename.ext",          "filename", "ext");
    test(6, ".ext",                   "",         "ext");
    test(7, "/path/to/filename",      "filename", "");
    test(8, "/path/to/filename.ext",  "filename", "ext");
    test(9, "./path/to/filename.ext", "filename", "ext");
    test(10, "/filename",              "filename", "");
    test(11, "/path/to/filename",      "filename", "");
    test(12, "/path/to/filename.",     "filename", "");
    test(13, "/path/to/.",             "",         "");
    test(14, "/path/to/.ext",          "",         "ext");
    test(15, "/path/to/filename.",     "filename", "");
    test(16, "filename.",              "filename", "");
    test(17, "./filename.",            "filename", "");

    return 0;
}
