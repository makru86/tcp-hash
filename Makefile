.PHONY: format-code loadtest coverage-report

up:
	@docker-compose up builder --force-recreate --remove-orphans  --build

down:
	@docker-compose down --remove-orphans

format-code:
	@docker-compose run builder bash -c \
		"find tcp_hash libtcp_hash -name \*.cpp -or -name \*.h | xargs clang-format -i"

tidy-code:
	@docker-compose run builder bash -c \
		"find tcp_hash libtcp_hash -name \*.cpp -or -name \*.h | head -1 | xargs clang-tidy -p build/ --export-fixes build/fixes.yaml --fix --fix-errors"

loadtest:
	@echo "The Pressure is Rising, The Adrenaline is Rushing, The Clock is Ticking"
	@docker-compose run builder \
		./build/libtcp_hash/loadtest

coverage-report:
	@docker-compose run builder gcovr --print-summary --xml build/coverage.xml --xml-pretty build
	@ls build/coverage.xml

prototype-up:
	@docker-compose -f docker-compose.yml -f compose-prototype.yaml up prototype

prototype-down:
	@docker-compose -f docker-compose.yml -f compose-prototype.yaml down --remove-orphans
