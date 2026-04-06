SOURCE           := main.c pa2.c
SRC_DIR          := src
BUILD_DIR        := build
EXECUTABLE       := pa2
EXECUTABLE_ASAN  := $(EXECUTABLE)-asan
PA2_SO           := lib$(EXECUTABLE).so

CC               := gcc
CFLAGS           := -Og -g -Isrc -std=c11 -Wall -Werror $(CFLAGS_AUTOGRADER)

.PHONY: run sanitize clean grade
.SUFFIXES: # no default implicit rules in the way

run: $(EXECUTABLE)
	./$(EXECUTABLE)

sanitize: $(EXECUTABLE_ASAN)
	./$(EXECUTABLE_ASAN)
#	$(info Sanitized build successful! Run ./$(EXECUTABLE_ASAN) to test your code with address sanitizer.)

valgrind: $(EXECUTABLE)
	valgrind --leak-check=full ./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)
	rm -f $(EXECUTABLE_ASAN)
	rm -rf $(BUILD_DIR)
	rm -f $(PA2_SO)

grade: clean
	$(warning [WARN] This Makefile target is only for the autograder! It will not do anything useful for a student.)
	$(CC) $(CFLAGS) $(SRC_DIR)/pa2.c -fdiagnostics-color=always -fPIE -shared -o $(PA2_SO)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

-include $(SOURCE:%.c=$(BUILD_DIR)/%.d)

#
# conventional build
#
# C compilation (C source -> binary object file)
$(SOURCE:%.c=$(BUILD_DIR)/%.o): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -MMD -MF $(@:%.o=%.d) $< -o $@

# Linking the object files ({all objects} -> executable binary)
$(EXECUTABLE): $(SOURCE:%.c=$(BUILD_DIR)/%.o) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@


#
# address-sanitizer build
#
# C compilation (C source -> binary object file)
$(SOURCE:%.c=$(BUILD_DIR)/%.o.asan): $(BUILD_DIR)/%.o.asan: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -fsanitize=address,undefined -MMD -MF $(@:%.o.asan=%.d) $< -o $@

# Linking the object files ({all objects} -> executable binary)
$(EXECUTABLE_ASAN): $(SOURCE:%.c=$(BUILD_DIR)/%.o.asan) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fsanitize=address,undefined $^ -o $@