MAJOR = 0
MINOR = 7
MICRO = 0
STATUS = ''

def current_version():
    return '%s.%s.%s%s' % (MAJOR, MINOR, MICRO, STATUS)

def current_version_tuple():
    return (MAJOR, MINOR, MICRO)

__version__ = current_version() 