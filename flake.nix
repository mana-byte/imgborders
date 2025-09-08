{
  description = "Build imgborders";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = {
    self,
    nixpkgs,
    flake-utils,
  }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
  in
    flake-utils.lib.eachDefaultSystem (system: let
      inherit (pkgs) stdenv;
    in {
      packages.imgborders = stdenv.mkDerivation rec {
        pname = "imgborders";
        version = builtins.readFile ./VERSION;

        src = ./.;

        nativeBuildInputs = [
          pkgs.cmake
          pkgs.pkg-config
          pkgs.libgcc
        ];

        # Add all pkg-config dependencies
        buildInputs = [
          pkgs.hyprland
          pkgs.libdrm
          pkgs.pixman
          pkgs.pango
          pkgs.cairo
          pkgs.glib
          pkgs.sysprof
          pkgs.fontconfig
          pkgs.xorg.libX11
          pkgs.xorg.libXdmcp
          pkgs.expat
          pkgs.pcre2

          pkgs.libinput
          pkgs.hyprgraphics
          pkgs.hyprutils
          pkgs.hyprcursor
          pkgs.hyprwayland-scanner
          pkgs.wayland
          pkgs.libxkbcommon
          pkgs.mesa
          pkgs.libglvnd

          pkgs.aquamarine
          pkgs.hyprlang
        ];

        cmakeFlags = [
          "-DCMAKE_CXX_STANDARD=23"
          "-DCMAKE_BUILD_TYPE=Release"
        ];

        meta = with pkgs.lib; {
          description = "Hyprland border effects library";
          homepage = "https://codeberg.org/zacoons/imgborders";
          license = licenses.mit;
          maintainers = [];
        };
      };
    });
}
