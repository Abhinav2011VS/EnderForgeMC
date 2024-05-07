{
  description = "A custom launcher for Minecraft that allows you to easily manage multiple installations of Minecraft at once";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-compat = { url = "github:edolstra/flake-compat"; flake = false; };
    libnbtplusplus = { url = "github:Abhinav2011VS/libnbtplusplus"; flake = false; };
    tomlplusplus = { url = "github:marzer/tomlplusplus"; flake = false; };
  };

  outputs = { self, nixpkgs, libnbtplusplus, tomlplusplus, ... }:
    let
      # User-friendly version number.
      version = builtins.substring 0 8 self.lastModifiedDate;

      # Supported systems (qtbase is currently broken for "aarch64-darwin")
      supportedSystems = [ "x86_64-linux" "x86_64-darwin" "aarch64-linux" ];

      # Helper function to generate an attrset '{ x86_64-linux = f "x86_64-linux"; ... }'.
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;

      # Nixpkgs instantiated for supported systems.
      pkgs = forAllSystems (system: nixpkgs.legacyPackages.${system});

      packagesFn = pkgs: rec {
        enderforgemc-unwrapped = pkgs.qt6Packages.callPackage ./nix/unwrapped.nix { inherit version self libnbtplusplus tomlplusplus; };
        enderforgemc-qt5-unwrapped = pkgs.libsForQt5.callPackage ./nix/unwrapped.nix { inherit version self libnbtplusplus tomlplusplus; };
        enderforgemc = pkgs.qt6Packages.callPackage ./nix { inherit version self enderforgemc-unwrapped; };
        enderforgemc-qt5 = pkgs.libsForQt5.callPackage ./nix { inherit version self; enderforgemc-unwrapped = enderforgemc-qt5-unwrapped; };
        default = enderforgemc;
      };
    in
    {
      packages = forAllSystems (system:
        let packages = packagesFn pkgs.${system}; in
        packages // { default = packages.enderforgemc; }
      );

      overlay = final: prev: packagesFn final;
    };
}
