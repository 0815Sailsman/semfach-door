from consts import *


class Band:
    def __init__(self):
        self.data = []
        self.standard_char = "#"
        self.position = 0

    def fill_with_std(self):
        self.data = list()
        for i in range(0, 100):
            self.data.append(self.standard_char)

    def set_standard_char(self, char):
        self.standard_char = char

    def set_position(self, new_position):
        self.position = new_position

    def read(self):
        return self.data[self.position]

    def write(self, new_char):
        self.data[self.position] = new_char

    def move(self, direction):
        if direction == NONE:
            pass
        elif direction == LEFT:
            self.position -= 1
        elif direction == RIGHT:
            self.position += 1

    def write_at_start(self, input):
        backup = self.position
        for i in input:
            self.data[self.position] = i
            self.position += 1
        self.position = backup
