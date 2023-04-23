.PHONY: format-code loadtest coverage-report

up:
	@echo "Building, testing, and starting service on 0.0.0.0:1234 in Docker..."
	@docker-compose up builder --force-recreate --remove-orphans  --build

down:
	@echo "The Pressure is Rising, The Adrenaline is Rushing, The Clock is Ticking"
	@docker-compose down --remove-orphans

format-code:
	@echo "Formatting code..."
	@docker-compose run builder bash -c \
		"find src test -name \*.cpp -or -name \*.h | xargs clang-format -i"

tidy-code:
	@echo "Running clang-tidy..."
	@docker-compose run builder bash -c \
		"find src test -name \*.cpp -or -name \*.h | head -1 | xargs clang-tidy -p build/ --export-fixes build/fixes.yaml --fix --fix-errors"

loadtest:
	@echo "Running loadtests..."
	@docker-compose run builder \
		./build/loadtest/loadtest

coverage-report:
	@echo "Generating coverage report..."
	@docker-compose run builder gcovr --print-summary --xml build/coverage.xml --xml-pretty build
	@ls build/coverage.xml

prototype-up:
	@echo "Starting prototype..."
	@docker-compose -f docker-compose.yml -f docker-compose.mock.yml up prototype

prototype-down:
	@echo "Stopping prototype..."
	@docker-compose -f docker-compose.yml -f docker-compose.mock.yml down --remove-orphans