from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout


class PL0Recipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def layout(self) -> None:
        cmake_layout(self)

    def requirements(self) -> None:
        self.requires("cli11/2.6.2")
        self.requires("fmt/12.1.0")

    def build_requirements(self) -> None:
        self.tool_requires("cmake/4.4.0")
        self.tool_requires("re2c/4.3")

    def build(self) -> None:
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
