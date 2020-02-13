from conans import ConanFile

class DCPPTaskMgr(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "gtest/1.8.0@bincrafters/stable",
        "spdlog/1.4.2@bincrafters/stable"
    )
    generators = "cmake"

    def imports(self):
        if self.settings.os == "Windows":
            if self.settings.build_type == "Debug":
                self.copy("*.dll", src="bin", dst="./Debug")
            else:  # Release
                self.copy("*.dll", src="bin", dst="./Release")
        else:  # Linux
            if self.settings.build_type == "Debug":
                self.copy("*.so*", src="lib", dst="./bin/shared-libs")
            else:  # Release
                self.copy("*.so*", src="lib", dst="./bin/shared-libs")
