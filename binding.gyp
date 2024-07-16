{
    "targets": [
        {
            "target_name": "FileZip",
            "include_dirs" : [
                "src",
                "<!(node -e \"require('nan')\")"
            ],
            "sources": [
                "src/index.cc",
                "src/FileZip.cc",
                "src/ZipStream.cc",
                "src/PathTrie.cc"
            ]
        }
    ],
}