plugins {
    `java-library`
}

description = "WinMerge plugin bridge module"

dependencies {
    api(project(":winmerge-core"))
}
