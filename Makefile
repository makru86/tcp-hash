.PHONY: format-code loadtest

format-code:
	@echo "Formatting code..."
	@docker-compose run builder \
		bash -c "find src test -name \*.cpp -or -name \*.hpp | xargs clang-format -i"

loadtest:
	@echo "Running loadtests..."
	@docker-compose run builder \
		./build/loadtest/loadtest