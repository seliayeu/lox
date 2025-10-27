{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };
  outputs = {nixpkgs, ...}: let
    pkgsDarwin = import nixpkgs { system = "aarch64-darwin"; };
    pkgsLinux = import nixpkgs { system = "x86_64-linux"; };
  in {
      devShells."aarch64-darwin".default = pkgsDarwin.mkShell.override { stdenv = pkgsDarwin.llvmPackages_19.stdenv; } {
        system = "aarch64-darwin";
        packages = with pkgsDarwin; [ 
          ninja
          cmake
          llvmPackages_19.clang-tools
        ];
        shellHook = ''
          export CPLUS_INCLUDE_PATH="${pkgsDarwin.llvmPackages_19.libcxx.dev}/include/c++/v1:$CPLUS_INCLUDE_PATH"
          export CMAKE_CXX_FLAGS="-stdlib=libc++ -I${pkgsDarwin.llvmPackages_19.libcxx.dev}/include/c++/v1"
        '';
      };
      devShells."x86_64-linux".default = pkgsLinux.mkShell.override { stdenv = pkgsLinux.llvmPackages_19.stdenv; } {
        system = "x86_64-linux";
        packages = with pkgsLinux; [ 
          ninja
          cmake
        ];
      };
    };
}
