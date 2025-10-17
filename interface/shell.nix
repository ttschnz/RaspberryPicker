{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.python312Full
    pkgs.tk
    pkgs.python312Packages.pip
    pkgs.python312Packages.tkinter
    pkgs.python312Packages.numpy
    pkgs.python312Packages.customtkinter
    pkgs.python312Packages.pyserial
    pkgs.python312Packages.matplotlib
  ];

}
