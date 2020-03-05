from conans import ConanFile, CMake, tools

class MflConan(ConanFile):
    name = "mfl"
    version = "1.0"
    url = "https://github.com/m-lima/mfl.git"
    description = "Util tools for cpp"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires = "fmt/4.1.0@bincrafters/stable"
    exports_sources = "include/mfl/*"

    def configure(self):
        self.options["fmt"].shared = False
        self.options["fmt"].header_only = True

    def package(self):
        self.copy("*.hpp", dst="include/mfl", src=self.source_folder, keep_path=False)

    def package_info(self):
        self.info.header_only()

