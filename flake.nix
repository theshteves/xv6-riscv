{
  description = "skOS";
  inputs.nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";

  outputs = { self, nixpkgs }: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
  in {
      devShells.${system}.default = pkgs.mkShell {
          packages = with pkgs; [
            coreboot-toolchain.riscv
            libgcc
            qemu
            perl
            ctags
            gnumake
            git
            vim
            gdb
            radare2
          ];
          
          shellHook = ''
            export PS1="\`if [ \$? = 0 ]; then echo \[\e[33m\]^_^\[\e[0m\]; else echo \[\e[31m\]O_O\[\e[0m\]; fi\` \[\033[0;32m\]\u \[\033[0m\]\w\[\033[m\]\[\033[0;32m\]\$ \[\033[0m\]"
          '';
      };
  };
}
