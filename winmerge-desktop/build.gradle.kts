import org.gradle.api.tasks.SourceSetContainer
import org.gradle.api.tasks.JavaExec

plugins {
    application
    id("org.openjfx.javafxplugin")
}

description = "WinMerge JavaFX desktop application module"

dependencies {
    implementation(project(":winmerge-core"))
    implementation("org.fxmisc.richtext:richtextfx:0.11.5")
    implementation("dev.secondsun:org.eclipse.tm4e.core:0.5.0")
    runtimeOnly(project(":winmerge-shell"))
    runtimeOnly(project(":winmerge-plugins"))

    testImplementation(platform("org.junit:junit-bom:5.11.4"))
    testImplementation("org.junit.jupiter:junit-jupiter")
}

javafx {
    version = "21.0.2"
    modules = listOf("javafx.controls", "javafx.fxml")
}

application {
    mainClass.set("org.winmerge.desktop.WinMergeApp")
}

tasks.register<JavaExec>("smokeTestUi") {
    group = "verification"
    description = "Launches a blank JavaFX window and exits automatically."
    dependsOn(tasks.named("classes"))

    val sourceSets = project.extensions.getByType(SourceSetContainer::class.java)
    classpath = sourceSets.getByName("main").runtimeClasspath
    mainClass.set(application.mainClass)
    args("--smoke-test")

    // javafxplugin configures --module-path on the run task outside of jvmArgs, so we
    // replicate it here: JavaFX JARs must be on the module path, not the classpath.
    doFirst {
        val javafxJars = classpath.filter { f -> f.name.startsWith("javafx-") }
        classpath = classpath.filter { f -> !f.name.startsWith("javafx-") }
        jvmArgs("--module-path", javafxJars.asPath, "--add-modules", "javafx.controls,javafx.fxml")
    }
}
