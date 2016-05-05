git_tree_sha := $(shell git rev-parse HEAD)
git_tree_sha_short := $(shell git rev-parse --short HEAD)
vasm_version := $(shell git describe --tags --long | sed 's/v//' | sed 's/-/./')

export GIT_TREE_SHA=$(git_tree_sha)
export GIT_TREE_SHA_SHORT=$(git_tree_sh_short)
export VASM_VERSION=$(vasm_version)

version.h.tmp:
	@echo "extern char git_tree_sha[];" > $@
	@echo "extern char git_tree_sha_short[];" >> $@
	@echo "extern char vasm_version[];" >> $@

version.h: version.h.tmp
	@if cmp -s $@ $@.tmp; then rm $@.tmp; else echo "Updated $@"; mv -f $@.tmp $@; fi

version.c.tmp:
	@echo "char git_tree_sha[] = \"$(git_tree_sha)\";" > $@
	@echo "char git_tree_sha_short[] = \"$(git_tree_sha_short)\";" >> $@
	@echo "char vasm_version[] = \"$(vasm_version)\";" >> $@

version.c: version.c.tmp version.h
	@if cmp -s $@ $@.tmp; then rm $@.tmp; else echo "Updated $@"; mv -f $@.tmp $@; fi

version_clean:
	rm -f version.c
	rm -f version.h

.PHONY: version_clean version.c.tmp

