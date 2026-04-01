plugins {
    `java-library`
}

description = "WinMerge plugin bridge module"

dependencies {
    api(project(":winmerge-core"))

    implementation("org.apache.commons:commons-compress:1.27.1")
    implementation("org.tukaani:xz:1.10")

    testImplementation(platform("org.junit:junit-bom:5.11.4"))
    testImplementation("org.junit.jupiter:junit-jupiter")
}
