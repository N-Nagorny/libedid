{
  run_tests ? false
}:

let
  pkgs = import <nixpkgs> {};
  build = pkgs.callPackage ./package.nix { inherit run_tests; };
in
  {
    inherit build;
    shell = pkgs.mkShell {
      inputsFrom = [ build ];
      packages = with pkgs; [
        cppcheck
        cpplint
        gitFull  # for "git gui"
        pre-commit
      ];
    };
  }
