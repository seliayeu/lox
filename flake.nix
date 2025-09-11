{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };
  outputs = {nixpkgs, ...}: let
    system = "aarch64-darwin";
    pkgs = import nixpkgs { inherit system; };
  in {
      devShells.${system}.default = pkgs.mkShell.override { stdenv = pkgs.llvmPackages_19.stdenv; } {
        inherit system;
        packages = with pkgs; [ 
          ninja
          cmake
          llvmPackages_19.clang-tools
        ];
        shellHook = ''
          export CPLUS_INCLUDE_PATH="${pkgs.llvmPackages_19.libcxx.dev}/include/c++/v1:$CPLUS_INCLUDE_PATH"
          export CMAKE_CXX_FLAGS="-stdlib=libc++ -I${pkgs.llvmPackages_19.libcxx.dev}/include/c++/v1"
        '';
      };
    };
}
