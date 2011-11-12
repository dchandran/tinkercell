"""
This wraps the TinkerCell API.
If it fails to load (when running outside TinkerCell),
then calling something with the @require_tinkercell
decorator will raise an APIError.
"""

import sys
try:
    sys.argv
except:
    sys.argv = ["fake arg to keep Tkinter from complaining"]

def printErrors(function):
    "generally useful for getting TinkerCell to show exceptions"
    try:
        function()
    except Exception, e:
        print type(e), e
        raise

class TinkerCellError(Exception): "error interacting with TinkerCell"
class APIError(TinkerCellError):  "error loading API methods"

try:
    from tinkercell import *
    from tc2py import *
    def require_tinkercell(whatever):
        return whatever
except ImportError:
    def require_tinkercell(whatever):
        def error():
            raise APIError
        return error

if __name__ == "__main__":
    @require_tinkercell
    def test_function():
        print "API Loaded."

    test_function()

