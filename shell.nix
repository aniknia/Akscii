{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
	buildInputs = [
		pkgs.gcc
		pkgs.gnumake
	];

	shellHook = ''
		echo "Starting C Shell Environment"
		echo "GCC version $(gcc -dumpversion)"
		echo "GNUMake version $(make --version | grep -oP 'GNU Make \K[0-9.]+')"
	'';
}
