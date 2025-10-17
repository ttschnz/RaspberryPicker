{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.python312Full
    pkgs.tk
    pkgs.python312Packages.pip
    pkgs.python312Packages.tkinter
    pkgs.python312Packages.numpy
  ];

  shellHook = ''
    if [ ! -d venv ]; then
      python -m venv venv
    fi
    source venv/bin/activate
    pip install -r requirements.txt
  '';
}
