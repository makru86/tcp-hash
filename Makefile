.PHONY: format-code loadtest coverage-report

format-code:
	@echo "Formatting code..."
	@docker-compose run builder \
		bash -c "find src test -name \*.cpp -or -name \*.hpp | xargs clang-format -i"

loadtest:
	@echo "Running loadtests..."
	@docker-compose run builder \
		./build/loadtest/loadtest

coverage-report:
	@echo "Generating coverage report..."
	@docker-compose run builder gcovr --xml build/coverage.xml build
	@ls build/coverage.xml