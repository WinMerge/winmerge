import org.gradle.api.plugins.JavaPlugin
import org.gradle.api.plugins.JavaPluginExtension
import org.gradle.api.tasks.compile.JavaCompile
import org.gradle.api.tasks.testing.Test
import org.gradle.jvm.toolchain.JavaLanguageVersion

plugins {
    base
    id("org.openjfx.javafxplugin") version "0.1.0" apply false
}

group = "org.winmerge"
version = "0.1.0-SNAPSHOT"

allprojects {
    repositories {
        mavenCentral()
    }
}

subprojects {
    group = rootProject.group
    version = rootProject.version

    plugins.withType<JavaPlugin> {
        extensions.getByType(JavaPluginExtension::class.java).apply {
            toolchain.languageVersion.set(JavaLanguageVersion.of(21))
            withSourcesJar()
        }

        tasks.withType(JavaCompile::class.java).configureEach {
            options.encoding = "UTF-8"
            options.release.set(17)
        }

        tasks.withType(Test::class.java).configureEach {
            useJUnitPlatform()
        }
    }
}
