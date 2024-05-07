# How to import

To import with flakes use

```nix
{
  inputs = {
    enderforgemc.url = "github:Abhinav2011VS/EnderForgeMC";
  };

...

  nixpkgs.overlays = [ inputs.enderforgemc.overlay ]; ## Within configuration.nix
  environment.systemPackages = with pkgs; [ enderforgemc ]; ##
}
```

To import without flakes use channels:

```sh
nix-channel --add https://github.com/Abhinav2011VS/EnderForgeMC/archive/master.tar.gz enderforgemc
nix-channel --update enderforgemc
nix-env -iA enderforgemc
```

or alternatively you can use

```nix
{
  nixpkgs.overlays = [
    (import (builtins.fetchTarball "https://github.com/Abhinav2011VS/EnderForgeMC/archive/develop.tar.gz")).overlay
  ];

  environment.systemPackages = with pkgs; [ enderforgemc ];
}
```
