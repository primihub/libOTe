load("@ladnir_cryptoTools//bazel:preload.bzl", "cryptoTools_preload")
load("@ladnir_cryptoTools//bazel:deps.bzl", "cryptoTools_deps")

def libote_deps():
  cryptoTools_preload("ladnir_cryptoTools")
  cryptoTools_deps()