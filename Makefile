.PHONY: build configure format coverage test serve docker-serve docker-test

MAKEFLAGS += --always-make

# optionally include .env file
-include .env
export

all: formatted build

configure:
	@cmake \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_MAKE_PROGRAM=ninja \
		-DCMAKE_C_COMPILER=clang \
		-DCMAKE_CXX_COMPILER=clang++ \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DBUILD_TESTS=ON \
		-DCODE_COVERAGE=OFF \
		-DASIO_ENABLE_HANDLER_TRACKING=OFF \
		-DENABLE_GPROF_PROFILING=OFF \
		-DENABLE_DEBUG_LOG=ON \
		-G Ninja \
		-S . \
		-B build

configure-release:
	@cmake \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_MAKE_PROGRAM=ninja \
		-DCMAKE_C_COMPILER=clang \
		-DCMAKE_CXX_COMPILER=clang++ \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DBUILD_TESTS=ON \
		-DCODE_COVERAGE=OFF \
		-DASIO_ENABLE_HANDLER_TRACKING=OFF \
		-DENABLE_GPROF_PROFILING=OFF \
		-DENABLE_DEBUG_LOG=OFF \
		-G Ninja \
		-S . \
		-B release


build: configure
	@cmake --build build -- -j 2

build-release: configure-release
	@cmake --build release -- -j 2

test:
	@ctest --test-dir build/libtcp_hash --output-on-failure

clean:
	@rm -rf build release

# up:
# 	@docker-compose up builder --force-recreate --remove-orphans  --build
#
# down:
# 	@docker-compose down --remove-orphans

formatted:
	@find tcp_hash libtcp_hash -name \*pp | xargs clang-format -i

serve: build-release
	@echo "tcp_hash is listening port 1234... Press Ctrl-C to stop"
	@./release/tcp_hash/tcp_hash

docker-test:
	@docker-compose run builder bash -c "make test"

docker-serve:
	@docker-compose run builder bash -c "make serve"

# formatted:
# 		"find tcp_hash libtcp_hash -name \*.cpp -or -name \*.h | xargs clang-format -i"
#
# tidy-code:
# 	@docker-compose run builder bash -c \
# 		"find tcp_hash libtcp_hash -name \*.cpp -or -name \*.h | head -1 | xargs clang-tidy -p build/ --export-fixes build/fixes.yaml --fix --fix-errors"
#
# loadtest:
# 	@echo "The Pressure is Rising, The Adrenaline is Rushing, The Clock is Ticking"
# 	@docker-compose run builder \
# 		./build/libtcp_hash/loadtest
#
# coverage-report:
# 	@docker-compose run builder gcovr --print-summary --xml build/coverage.xml --xml-pretty build
# 	@ls build/coverage.xml
#
# prototype-up:
# 	@docker-compose -f docker-compose.yml -f compose-prototype.yaml up prototype
#
# prototype-down:
# 	@docker-compose -f docker-compose.yml -f compose-prototype.yaml down --remove-orphans
