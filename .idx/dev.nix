{ pkgs, ... }: {
  # Which nixpkgs channel to use.
  channel = "stable-24.05"; # or "unstable"

  # Use https://search.nixos.org/packages to find packages
  packages = [
    # pkgs.go
 # pkgs.python311
 # pkgs.python311Packages.pip
 # pkgs.nodejs_20
    pkgs.make
  ];
}