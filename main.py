from band import Band
from consts import *


class TM:
    def __init__(self):
        self.state = 0
        self.band = Band()
        self.band.set_standard_char("#")
        self.band.fill_with_std()

    def execute(self, input):
        self.band.set_position(1)
        self.band.write_at_start(input)
        final_states = [42]
        while self.state is not None:
            #print("State+" + str(self.state))
            read_value = self.band.read()
            #print("read_value:" + str(read_value))
            write_value = self.get_write_value(self.state, read_value)
            #print("write:" + str(write_value))
            if not write_value:
                return False
            next_state = self.get_next_state(self.state, read_value)
            if next_state in final_states:
                return True
            if not next_state:
                return False
            next_move = self.get_next_move(self.state, read_value)
            #print("move" + next_move)
            self.band.write(write_value)
            self.state = next_state
            self.band.move(next_move)

    def get_write_value(self, state, read):
        if state == 0:
            if read == "0":
                return "0"
            elif read == "1":
                return "1"
            elif read == "2":
                return "2"
            elif read == "3":
                return "3"
            elif read == "4":
                return "4"
            elif read == "5":
                return "5"
            elif read == "6":
                return "6"
            elif read == "7":
                return "7"
            elif read == "8":
                return "8"
            elif read == "9":
                return "9"
        elif state == 1:
            if read == "0":
                return "0"
            elif read == "1":
                return "1"
            elif read == "2":
                return "2"
            elif read == "3":
                return "3"
            elif read == "4":
                return "4"
            elif read == "5":
                return "5"
            elif read == "6":
                return "6"
            elif read == "7":
                return "7"
            elif read == "8":
                return "8"
            elif read == "9":
                return "9"
        elif state == 2:
            if read == "0":
                return "0"
            elif read == "1":
                return "1"
            elif read == "2":
                return "2"
            elif read == "3":
                return "3"
            elif read == "4":
                return "4"
            elif read == "5":
                return "5"
            elif read == "6":
                return "6"
            elif read == "7":
                return "7"
            elif read == "8":
                return "8"
            elif read == "9":
                return "9"
        elif state == 3:
            if read == "0":
                return "0"
            elif read == "1":
                return "1"
            elif read == "2":
                return "2"
            elif read == "3":
                return "3"
            elif read == "4":
                return "4"
            elif read == "5":
                return "5"
            elif read == "6":
                return "6"
            elif read == "7":
                return "7"
            elif read == "8":
                return "8"
            elif read == "9":
                return "9"
        elif state == 4:
            if read == "0":
                return "0"
            elif read == "1":
                return "1"
            elif read == "2":
                return "2"
            elif read == "3":
                return "3"
            elif read == "4":
                return "4"
            elif read == "5":
                return "5"
            elif read == "6":
                return "6"
            elif read == "7":
                return "7"
            elif read == "8":
                return "8"
            elif read == "9":
                return "9"
        elif state == 5:
            if read == "#":
                return "|"
        elif state == 6:
            if read == "#":
                return "0"
        elif state ==7:
            if read == "#":
                return "0"
        elif state == 8:
            if read == "#":
                return "0"
        elif state == 9:
            if read == "#":
                return "0"
        elif state == 10:
            if read == "#":
                return "0"
        elif state == 11:
            if read == "#":
                return "0"
        elif state == 12:
            if read == "#":
                return "|"
        elif state == 13:
            if read == "|":
                return "|"
            elif read == "0":
                return "0"
            elif read == "1":
                return "1"
        elif state == 14:
            if read == "0":
                return "0"
            elif read == "1":
                return "1"
            elif read == "2":
                return "2"
            elif read == "3":
                return "3"
            elif read == "4":
                return "4"
            elif read == "5":
                return "5"
            elif read == "6":
                return "6"
            elif read == "7":
                return "7"
            elif read == "8":
                return "8"
            elif read == "9":
                return "9"
        elif state == 15:
            if read == "0":
                return "0"
            elif read == "1":
                return "0"
            elif read == "2":
                return "1"
            elif read == "3":
                return "2"
            elif read == "4":
                return "3"
            elif read == "5":
                return "4"
            elif read == "6":
                return "5"
            elif read == "7":
                return "6"
            elif read == "8":
                return "7"
            elif read == "9":
                return "8"
        elif state == 16:
            if read == "#":
                return "#"
            elif read == "0":
                return "0"
            elif read == "1":
                return "0"
            elif read == "2":
                return "1"
            elif read == "3":
                return "2"
            elif read == "4":
                return "3"
            elif read == "5":
                return "4"
            elif read == "6":
                return "5"
            elif read == "7":
                return "6"
            elif read == "8":
                return "7"
            elif read == "9":
                return "8"
        elif state == 17:
            if read == "|":
                return "|"
            elif read == "0":
                return "0"
            elif read == "1":
                return "1"
            elif read == "2":
                return "2"
            elif read == "3":
                return "3"
            elif read == "4":
                return "4"
            elif read == "5":
                return "5"
            elif read == "6":
                return "6"
            elif read == "7":
                return "7"
            elif read == "8":
                return "8"
            elif read == "9":
                return "9"
        elif state == 18:
            if read == "0":
                return "1"
            elif read == "1":
                return "0"
        elif state == 19:
            if read == "#":
                return "0"
            elif read == "|":
                return "|"
            elif read == "0":
                return "0"
            elif read == "1":
                return "1"
            elif read == "2":
                return "2"
            elif read == "3":
                return "3"
            elif read == "4":
                return "4"
            elif read == "5":
                return "5"
            elif read == "6":
                return "6"
            elif read == "7":
                return "7"
            elif read == "8":
                return "8"
            elif read == "9":
                return "9"
        elif state == 20:
            if read == "#":
                return "0"
        elif state == 21:
            if read == "#":
                return "|"
        elif state == 22:
            if read == "|":
                return "|"
            elif read == "0":
                return "0"
            elif read == "1":
                return "1"
            elif read == "2":
                return "2"
            elif read == "3":
                return "3"
            elif read == "4":
                return "4"
            elif read == "5":
                return "5"
            elif read == "6":
                return "6"
            elif read == "7":
                return "7"
            elif read == "8":
                return "8"
            elif read == "9":
                return "9"
        elif state == 23:
            if read == "|":
                return "|"
            elif read == "0":
                return "0"
            elif read == "1":
                return "0"
        elif state == 24:
            if read == "|":
                return "|"
            elif read == "0":
                return "1"
        elif state == 25:
            if read == "0":
                return "0"
            elif read == "1":
                return "1"
            elif read == "2":
                return "2"
            elif read == "3":
                return "3"
            elif read == "4":
                return "4"
            elif read == "5":
                return "5"
            elif read == "6":
                return "6"
            elif read == "7":
                return "7"
            elif read == "8":
                return "8"
            elif read == "9":
                return "9"
        elif state == 26:
            if read == "0":
                return "1"
            elif read == "1":
                return "2"
            elif read == "2":
                return "3"
            elif read == "3":
                return "4"
            elif read == "4":
                return "5"
            elif read == "5":
                return "6"
            elif read == "6":
                return "7"
            elif read == "7":
                return "8"
            elif read == "8":
                return "9"
            elif read == "9":
                return "0"
        elif state == 27:
            if read == "0":
                return "1"
            elif read == "1":
                return "2"
            elif read == "2":
                return "3"
            elif read == "3":
                return "4"
        elif state == 28:
            if read == "|":
                return "|"
            elif read == "0":
                return "0"
        elif state == 29:
            if read == "0":
                return "0"
            elif read == "1":
                return "0"
            elif read == "2":
                return "1"
            elif read == "3":
                return "2"
            elif read == "4":
                return "3"
            elif read == "5":
                return "4"
            elif read == "6":
                return "5"
            elif read == "7":
                return "6"
            elif read == "8":
                return "7"
            elif read == "9":
                return "8"
        elif state == 30:
            if read == "0":
                return "0"
            elif read == "1":
                return "0"
            elif read == "2":
                return "1"
            elif read == "3":
                return "2"
            elif read == "4":
                return "3"
            elif read == "5":
                return "4"
            elif read == "6":
                return "5"
            elif read == "7":
                return "6"
            elif read == "8":
                return "7"
            elif read == "9":
                return "8"
        elif state == 31:
            if read == "0":
                return "1"
            elif read == "1":
                return "2"
            elif read == "2":
                return "3"
            elif read == "3":
                return "4"
            elif read == "4":
                return "5"
            elif read == "5":
                return "6"
            elif read == "6":
                return "7"
            elif read == "7":
                return "8"
            elif read == "8":
                return "9"
            elif read == "9":
                return "0"
        elif state == 32:
            if read == "0":
                return "1"
            elif read == "1":
                return "2"
            elif read == "2":
                return "3"
            elif read == "3":
                return "4"
            elif read == "4":
                return "5"
            elif read == "5":
                return "6"
            elif read == "6":
                return "7"
            elif read == "7":
                return "8"
            elif read == "8":
                return "9"
        elif state == 33:
            if read == "0":
                return "0"
        elif state == 34:
            if read == "0":
                return "0"
        elif state == 35:
            if read == "|":
                return "|"
        elif state == 36:
            if read == "|":
                return "|"
            elif read == "0":
                return "0"
        elif state == 37:
            if read == "1":
                return "0"
            elif read == "2":
                return "1"
            elif read == "3":
                return "2"
            elif read == "4":
                return "3"
            elif read == "5":
                return "4"
            elif read == "6":
                return "5"
            elif read == "7":
                return "6"
            elif read == "8":
                return "7"
            elif read == "9":
                return "8"
        elif state == 38:
            if read == "#":
                return "#"
            elif read == "|":
                return "|"
            elif read == "0":
                return "0"
            elif read == "1":
                return "1"
            elif read == "2":
                return "2"
            elif read == "3":
                return "3"
            elif read == "4":
                return "4"
            elif read == "5":
                return "5"
            elif read == "6":
                return "6"
            elif read == "7":
                return "7"
            elif read == "8":
                return "8"
            elif read == "9":
                return "9"
        elif state == 39:
            if read == "|":
                return "|"
        elif state == 40:
            if read == "|":
                return "|"
            elif read == "0":
                return "0"
        elif state == 41:
            if read == "0":
                return "0"
        elif state == 42:
            pass
        elif state == 43:
            if read == "|":
                return "|"
        elif state == 44:
            if read == "|":
                return "|"
            elif read == "0":
                return "0"
            elif read == "1":
                return "1"
        return False

    def get_next_state(self, state, read):
        if state == 0:
            if read == "0":
                return 1
            elif read == "1":
                return 1
            elif read == "2":
                return 1
            elif read == "3":
                return 1
            elif read == "4":
                return 1
            elif read == "5":
                return 1
            elif read == "6":
                return 1
            elif read == "7":
                return 1
            elif read == "8":
                return 1
            elif read == "9":
                return 1
        elif state == 1:
            if read == "0":
                return 2
            elif read == "1":
                return 2
            elif read == "2":
                return 2
            elif read == "3":
                return 2
            elif read == "4":
                return 2
            elif read == "5":
                return 2
            elif read == "6":
                return 2
            elif read == "7":
                return 2
            elif read == "8":
                return 2
            elif read == "9":
                return 2
        elif state == 2:
            if read == "0":
                return 3
            elif read == "1":
                return 3
            elif read == "2":
                return 3
            elif read == "3":
                return 3
            elif read == "4":
                return 3
            elif read == "5":
                return 3
            elif read == "6":
                return 3
            elif read == "7":
                return 3
            elif read == "8":
                return 3
            elif read == "9":
                return 3
        elif state == 3:
            if read == "0":
                return 4
            elif read == "1":
                return 4
            elif read == "2":
                return 4
            elif read == "3":
                return 4
            elif read == "4":
                return 4
            elif read == "5":
                return 4
            elif read == "6":
                return 4
            elif read == "7":
                return 4
            elif read == "8":
                return 4
            elif read == "9":
                return 4
        elif state == 4:
            if read == "0":
                return 5
            elif read == "1":
                return 5
            elif read == "2":
                return 5
            elif read == "3":
                return 5
            elif read == "4":
                return 5
            elif read == "5":
                return 5
            elif read == "6":
                return 5
            elif read == "7":
                return 5
            elif read == "8":
                return 5
            elif read == "9":
                return 5
        elif state == 5:
            if read == "#":
                return 6
        elif state == 6:
            if read == "#":
                return 7
        elif state ==7:
            if read == "#":
                return 8
        elif state == 8:
            if read == "#":
                return 9
        elif state == 9:
            if read == "#":
                return 10
        elif state == 10:
            if read == "#":
                return 11
        elif state == 11:
            if read == "#":
                return 12
        elif state == 12:
            if read == "#":
                return 13
        elif state == 13:
            if read == "|":
                return 14
            elif read == "0":
                return 13
            elif read == "1":
                return 13
        elif state == 14:
            if read == "0":
                return 15
            elif read == "1":
                return 15
            elif read == "2":
                return 15
            elif read == "3":
                return 15
            elif read == "4":
                return 15
            elif read == "5":
                return 15
            elif read == "6":
                return 15
            elif read == "7":
                return 15
            elif read == "8":
                return 15
            elif read == "9":
                return 15
        elif state == 15:
            if read == "0":
                return 16
            elif read == "1":
                return 17
            elif read == "2":
                return 17
            elif read == "3":
                return 17
            elif read == "4":
                return 17
            elif read == "5":
                return 17
            elif read == "6":
                return 17
            elif read == "7":
                return 17
            elif read == "8":
                return 17
            elif read == "9":
                return 17
        elif state == 16:
            if read == "#":
                return 19
            elif read == "0":
                return 16
            elif read == "1":
                return 17
            elif read == "2":
                return 17
            elif read == "3":
                return 17
            elif read == "4":
                return 17
            elif read == "5":
                return 17
            elif read == "6":
                return 17
            elif read == "7":
                return 17
            elif read == "8":
                return 17
            elif read == "9":
                return 17
        elif state == 17:
            if read == "|":
                return 44
            elif read == "0":
                return 17
            elif read == "1":
                return 17
            elif read == "2":
                return 17
            elif read == "3":
                return 17
            elif read == "4":
                return 17
            elif read == "5":
                return 17
            elif read == "6":
                return 17
            elif read == "7":
                return 17
            elif read == "8":
                return 17
            elif read == "9":
                return 17
        elif state == 18:
            if read == "0":
                return 13
            elif read == "1":
                return 18
        elif state == 19:
            if read == "#":
                return 20
            elif read == "|":
                return 19
            elif read == "0":
                return 19
            elif read == "1":
                return 19
            elif read == "2":
                return 19
            elif read == "3":
                return 19
            elif read == "4":
                return 19
            elif read == "5":
                return 19
            elif read == "6":
                return 19
            elif read == "7":
                return 19
            elif read == "8":
                return 19
            elif read == "9":
                return 19
        elif state == 20:
            if read == "#":
                return 21
        elif state == 21:
            if read == "#":
                return 22
        elif state == 22:
            if read == "|":
                return 23
            elif read == "0":
                return 22
            elif read == "1":
                return 22
            elif read == "2":
                return 22
            elif read == "3":
                return 22
            elif read == "4":
                return 22
            elif read == "5":
                return 22
            elif read == "6":
                return 22
            elif read == "7":
                return 22
            elif read == "8":
                return 22
            elif read == "9":
                return 22
        elif state == 23:
            if read == "|":
                return 28
            elif read == "0":
                return 23
            elif read == "1":
                return 24
        elif state == 24:
            if read == "|":
                return 25
            elif read == "0":
                return 24
        elif state == 25:
            if read == "0":
                return 26
            elif read == "1":
                return 26
            elif read == "2":
                return 26
            elif read == "3":
                return 26
            elif read == "4":
                return 26
            elif read == "5":
                return 26
            elif read == "6":
                return 26
            elif read == "7":
                return 26
            elif read == "8":
                return 26
            elif read == "9":
                return 26
        elif state == 26:
            if read == "0":
                return 22
            elif read == "1":
                return 22
            elif read == "2":
                return 22
            elif read == "3":
                return 22
            elif read == "4":
                return 22
            elif read == "5":
                return 22
            elif read == "6":
                return 22
            elif read == "7":
                return 22
            elif read == "8":
                return 22
            elif read == "9":
                return 27
        elif state == 27:
            if read == "0":
                return 22
            elif read == "1":
                return 22
            elif read == "2":
                return 22
            elif read == "3":
                return 22
        elif state == 28:
            if read == "|":
                return 29
            elif read == "0":
                return 28
        elif state == 29:
            if read == "0":
                return 30
            elif read == "1":
                return 31
            elif read == "2":
                return 31
            elif read == "3":
                return 31
            elif read == "4":
                return 31
            elif read == "5":
                return 31
            elif read == "6":
                return 31
            elif read == "7":
                return 31
            elif read == "8":
                return 31
            elif read == "9":
                return 31
        elif state == 30:
            if read == "0":
                return 34
            elif read == "1":
                return 33
            elif read == "2":
                return 33
            elif read == "3":
                return 33
            elif read == "4":
                return 33
            elif read == "5":
                return 33
            elif read == "6":
                return 33
            elif read == "7":
                return 33
            elif read == "8":
                return 33
            elif read == "9":
                return 33
        elif state == 31:
            if read == "0":
                return 29
            elif read == "1":
                return 29
            elif read == "2":
                return 29
            elif read == "3":
                return 29
            elif read == "4":
                return 29
            elif read == "5":
                return 29
            elif read == "6":
                return 29
            elif read == "7":
                return 29
            elif read == "8":
                return 29
            elif read == "9":
                return 32
        elif state == 32:
            if read == "0":
                return 29
            elif read == "1":
                return 29
            elif read == "2":
                return 29
            elif read == "3":
                return 29
            elif read == "4":
                return 29
            elif read == "5":
                return 29
            elif read == "6":
                return 29
            elif read == "7":
                return 29
            elif read == "8":
                return 29
        elif state == 33:
            if read == "0":
                return 35
        elif state == 34:
            if read == "0":
                return 39
        elif state == 35:
            if read == "|":
                return 36
        elif state == 36:
            if read == "|":
                return 37
            elif read == "0":
                return 36
        elif state == 37:
            if read == "1":
                return 38
            elif read == "2":
                return 38
            elif read == "3":
                return 38
            elif read == "4":
                return 38
            elif read == "5":
                return 38
            elif read == "6":
                return 38
            elif read == "7":
                return 38
            elif read == "8":
                return 38
            elif read == "9":
                return 38
        elif state == 38:
            if read == "#":
                return 43
            elif read == "|":
                return 38
            elif read == "0":
                return 38
            elif read == "1":
                return 38
            elif read == "2":
                return 38
            elif read == "3":
                return 38
            elif read == "4":
                return 38
            elif read == "5":
                return 38
            elif read == "6":
                return 38
            elif read == "7":
                return 38
            elif read == "8":
                return 38
            elif read == "9":
                return 38
        elif state == 39:
            if read == "|":
                return 40
        elif state == 40:
            if read == "|":
                return 41
            elif read == "0":
                return 40
        elif state == 41:
            if read == "0":
                return 42
        elif state == 42:
            pass
        elif state == 43:
            if read == "|":
                return 30
        elif state == 44:
            if read == "|":
                return 18
            elif read == "0":
                return 44
            elif read == "1":
                return 44
        return False

    def get_next_move(self, state, read):
        if state == 0:
            if read == "0":
                return "R"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 1:
            if read == "0":
                return "R"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 2:
            if read == "0":
                return "R"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 3:
            if read == "0":
                return "R"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 4:
            if read == "0":
                return "R"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 5:
            if read == "#":
                return "R"
        elif state == 6:
            if read == "#":
                return "R"
        elif state ==7:
            if read == "#":
                return "R"
        elif state == 8:
            if read == "#":
                return "R"
        elif state == 9:
            if read == "#":
                return "R"
        elif state == 10:
            if read == "#":
                return "R"
        elif state == 11:
            if read == "#":
                return "R"
        elif state == 12:
            if read == "#":
                return "L"
        elif state == 13:
            if read == "|":
                return "L"
            elif read == "0":
                return "L"
            elif read == "1":
                return "L"
        elif state == 14:
            if read == "0":
                return "L"
            elif read == "1":
                return "L"
            elif read == "2":
                return "L"
            elif read == "3":
                return "L"
            elif read == "4":
                return "L"
            elif read == "5":
                return "L"
            elif read == "6":
                return "L"
            elif read == "7":
                return "L"
            elif read == "8":
                return "L"
            elif read == "9":
                return "L"
        elif state == 15:
            if read == "0":
                return "L"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 16:
            if read == "#":
                return "R"
            elif read == "0":
                return "L"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 17:
            if read == "|":
                return "R"
            elif read == "0":
                return "R"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 18:
            if read == "0":
                return "L"
            elif read == "1":
                return "L"
        elif state == 19:
            if read == "#":
                return "R"
            elif read == "|":
                return "R"
            elif read == "0":
                return "R"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 20:
            if read == "#":
                return "R"
        elif state == 21:
            if read == "#":
                return "L"
        elif state == 22:
            if read == "|":
                return "L"
            elif read == "0":
                return "L"
            elif read == "1":
                return "L"
            elif read == "2":
                return "L"
            elif read == "3":
                return "L"
            elif read == "4":
                return "L"
            elif read == "5":
                return "L"
            elif read == "6":
                return "L"
            elif read == "7":
                return "L"
            elif read == "8":
                return "L"
            elif read == "9":
                return "L"
        elif state == 23:
            if read == "|":
                return "R"
            elif read == "0":
                return "L"
            elif read == "1":
                return "R"
        elif state == 24:
            if read == "|":
                return "R"
            elif read == "0":
                return "R"
        elif state == 25:
            if read == "0":
                return "R"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 26:
            if read == "0":
                return "L"
            elif read == "1":
                return "L"
            elif read == "2":
                return "L"
            elif read == "3":
                return "L"
            elif read == "4":
                return "L"
            elif read == "5":
                return "L"
            elif read == "6":
                return "L"
            elif read == "7":
                return "L"
            elif read == "8":
                return "L"
            elif read == "9":
                return "L"
        elif state == 27:
            if read == "0":
                return "L"
            elif read == "1":
                return "L"
            elif read == "2":
                return "L"
            elif read == "3":
                return "L"
        elif state == 28:
            if read == "|":
                return "R"
            elif read == "0":
                return "R"
        elif state == 29:
            if read == "0":
                return "R"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 30:
            if read == "0":
                return "L"
            elif read == "1":
                return "L"
            elif read == "2":
                return "L"
            elif read == "3":
                return "L"
            elif read == "4":
                return "L"
            elif read == "5":
                return "L"
            elif read == "6":
                return "L"
            elif read == "7":
                return "L"
            elif read == "8":
                return "L"
            elif read == "9":
                return "L"
        elif state == 31:
            if read == "0":
                return "L"
            elif read == "1":
                return "L"
            elif read == "2":
                return "L"
            elif read == "3":
                return "L"
            elif read == "4":
                return "L"
            elif read == "5":
                return "L"
            elif read == "6":
                return "L"
            elif read == "7":
                return "L"
            elif read == "8":
                return "L"
            elif read == "9":
                return "L"
        elif state == 32:
            if read == "0":
                return "N"
            elif read == "1":
                return "N"
            elif read == "2":
                return "N"
            elif read == "3":
                return "N"
            elif read == "4":
                return "N"
            elif read == "5":
                return "N"
            elif read == "6":
                return "N"
            elif read == "7":
                return "N"
            elif read == "8":
                return "N"
        elif state == 33:
            if read == "0":
                return "L"
        elif state == 34:
            if read == "0":
                return "L"
        elif state == 35:
            if read == "|":
                return "L"
        elif state == 36:
            if read == "|":
                return "L"
            elif read == "0":
                return "L"
        elif state == 37:
            if read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 38:
            if read == "#":
                return "L"
            elif read == "|":
                return "R"
            elif read == "0":
                return "R"
            elif read == "1":
                return "R"
            elif read == "2":
                return "R"
            elif read == "3":
                return "R"
            elif read == "4":
                return "R"
            elif read == "5":
                return "R"
            elif read == "6":
                return "R"
            elif read == "7":
                return "R"
            elif read == "8":
                return "R"
            elif read == "9":
                return "R"
        elif state == 39:
            if read == "|":
                return "L"
        elif state == 40:
            if read == "|":
                return "L"
            elif read == "0":
                return "L"
        elif state == 41:
            if read == "0":
                return "N"
        elif state == 42:
            pass
        elif state == 43:
            if read == "|":
                return "L"
        elif state == 44:
            if read == "|":
                return "L"
            elif read == "0":
                return "R"
            elif read == "1":
                return "R"
        return False


automat = TM()
result = automat.execute(["8", "8", "8", "8", "5"])
if result:
    print("Der Code funktioniert!")
else:
    print("Der Code funktioniert nicht!")
