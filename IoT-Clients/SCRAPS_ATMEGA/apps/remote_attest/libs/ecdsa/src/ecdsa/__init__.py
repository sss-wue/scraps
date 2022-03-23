# while we don't use six in this file, we did bundle it for a long time, so
# keep as part of module in a virtual way (through __all__)
from . import six
from .keys import (
    SigningKey,
    VerifyingKey,
    BadSignatureError,
    BadDigestError,
    MalformedPointError,
)
from .curves import (
    NIST192p,
    NIST224p,
    NIST256p,
    NIST384p,
    NIST521p,
    SECP256k1,
    BRAINPOOLP160r1,
    BRAINPOOLP192r1,
    BRAINPOOLP224r1,
    BRAINPOOLP256r1,
    BRAINPOOLP320r1,
    BRAINPOOLP384r1,
    BRAINPOOLP512r1,
    SECP112r1,
    SECP112r2,
    SECP128r1,
    SECP160r1,
)
from .ecdh import (
    ECDH,
    NoKeyError,
    NoCurveError,
    InvalidCurveError,
    InvalidSharedSecretError,
)
from .der import UnexpectedDER

# This code comes from http://github.com/tlsfuzzer/python-ecdsa
from ._version import get_versions

__version__ = get_versions()["version"]
del get_versions

__all__ = [
    "curves",
    "der",
    "ecdsa",
    "ellipticcurve",
    "keys",
    "numbertheory",
    "test_pyecdsa",
    "util",
    "six",
]

_hush_pyflakes = [
    SigningKey,
    VerifyingKey,
    BadSignatureError,
    BadDigestError,
    MalformedPointError,
    UnexpectedDER,
    InvalidCurveError,
    NoKeyError,
    InvalidSharedSecretError,
    ECDH,
    NoCurveError,
    NIST192p,
    NIST224p,
    NIST256p,
    NIST384p,
    NIST521p,
    SECP256k1,
    BRAINPOOLP160r1,
    BRAINPOOLP192r1,
    BRAINPOOLP224r1,
    BRAINPOOLP256r1,
    BRAINPOOLP320r1,
    BRAINPOOLP384r1,
    BRAINPOOLP512r1,
    SECP112r1,
    SECP112r2,
    SECP128r1,
    SECP160r1,
    six.b(""),
]
del _hush_pyflakes
