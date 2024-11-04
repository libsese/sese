#  Copyright 2024 libsese
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

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
