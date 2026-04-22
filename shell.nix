{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
	buildInputs = [
		pkgs.gcc
		pkgs.gnumake

		# Python
		(pkgs.python3.withPackages(p: with p; [
			numpy
			]
		))
	];

	shellHook = ''
		echo "Starting C/Python Shell Environment"
		echo "GCC version $(gcc -dumpversion)"
		echo "GNUMake version $(make --version | grep -oP 'GNU Make \K[0-9.]+')"
		echo "Python version $(python --version)"
	'';
}
