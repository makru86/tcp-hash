format-code:
	@echo "Formatting code..."
	@docker-compose run builder \
		bash -c "find src test -name \*.cpp -or -name \*.hpp | xargs clang-format -i"