plugins {
    `java-library`
}

description = "WinMerge core diff/merge engine module"

dependencies {
    testImplementation(platform("org.junit:junit-bom:5.11.4"))
    testImplementation("org.junit.jupiter:junit-jupiter")
}
