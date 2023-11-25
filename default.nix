let
  pkgs = import <nixpkgs> {};
  build = pkgs.callPackage ./package.nix {};
in
  {
    inherit build;
    shell = pkgs.mkShell {
      inputsFrom = [ build ];
      packages = with pkgs; [
        cppcheck
        cpplint
      ];
    };
  }
