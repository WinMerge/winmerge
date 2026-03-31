import org.gradle.api.tasks.SourceSetContainer
import org.gradle.api.tasks.JavaExec

plugins {
    application
    id("org.openjfx.javafxplugin")
}

description = "WinMerge JavaFX desktop application module"

dependencies {
    implementation(project(":winmerge-core"))
    runtimeOnly(project(":winmerge-shell"))
    runtimeOnly(project(":winmerge-plugins"))
}

javafx {
    version = "21.0.2"
    modules = listOf("javafx.controls", "javafx.fxml")
}

application {
    mainClass.set("org.winmerge.desktop.WinMergeDesktopApp")
}

tasks.register<JavaExec>("smokeTestUi") {
    group = "verification"
    description = "Launches a blank JavaFX window and exits automatically."
    dependsOn(tasks.named("classes"))

    val sourceSets = extensions.getByType(SourceSetContainer::class.java)
    classpath = sourceSets.getByName("main").runtimeClasspath
    mainClass.set(application.mainClass)
    args("--smoke-test")
}
