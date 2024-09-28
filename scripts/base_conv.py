import unittest

BASE62 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"


def encode(num, alphabet=BASE62):
    if num == 0:
        return alphabet[0]
    arr = []
    base = len(alphabet)
    while num:
        num, rem = divmod(num, base)
        arr.append(alphabet[rem])
    arr.reverse()
    return "".join(arr)


def decode(string, alphabet=BASE62):
    base = len(alphabet)
    strlen = len(string)
    num = 0
    idx = 0
    for char in string:
        power = strlen - (idx + 1)
        num += alphabet.index(char) * (base**power)
        idx += 1

    return num


class ConvTest(unittest.TestCase):
    def test_decode_integer(self):
        res = decode("ftXl")
        self.assertEqual(res, 7562611)

    def test_encode_buffer(self):
        res = encode(7562611)
        self.assertEqual(res, "ftXl")


if __name__ == "__main__":
    unittest.main()
