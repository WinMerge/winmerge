plugins {
    `java-library`
}

description = "WinMerge shell integration module"

dependencies {
    api(project(":winmerge-core"))

    testImplementation(platform("org.junit:junit-bom:5.11.4"))
    testImplementation("org.junit.jupiter:junit-jupiter")
}
