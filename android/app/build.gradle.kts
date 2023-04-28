import org.gradle.crypto.checksum.Checksum

plugins {
    id("com.android.application")
    id("org.gradle.crypto.checksum") version "1.4.0"
}

java {
    toolchain {
        languageVersion.set(JavaLanguageVersion.of(11))
    }
}

android {
    compileSdk = 33
    buildToolsVersion = "33.0.2"
    ndkVersion = "25.2.9519653"
    namespace = "com.reecedunn.espeak"

    defaultConfig {
        applicationId = "com.reecedunn.espeak"
        minSdk = 19
        targetSdk = 33
        versionCode = 22
        versionName = "1.52-dev"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        testApplicationId = "com.reecedunn.espeak.test"
        externalNativeBuild {
            cmake {
                arguments += listOf(
                    "-DUSE_ASYNC:BOOL=OFF",
                    "-DUSE_MBROLA:BOOL=OFF"
                )
                targets += listOf(
                    "ttsespeak",
                    "espeak-data"
                )
            }
        }
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    lint {
        abortOnError = false
    }

    compileOptions {
        sourceCompatibility(JavaVersion.VERSION_11)
        targetCompatibility(JavaVersion.VERSION_11)
    }
    buildFeatures {
        buildConfig = true
        viewBinding = true
    }
}

dependencies {
    testImplementation("junit:junit:4.13.2")
    androidTestImplementation("com.google.android:android-test:4.1.1.4")
    androidTestImplementation("org.hamcrest:hamcrest-all:1.3")
}

tasks.register("checkData") {
    doFirst {
        assert(file("../../espeak-ng-data/en_dict").exists())
        assert(file("../../espeak-ng-data/intonations").exists())
        assert(file("../../espeak-ng-data/phondata").exists())
        assert(file("../../espeak-ng-data/phondata-manifest").exists())
        assert(file("../../espeak-ng-data/phonindex").exists())
        assert(file("../../espeak-ng-data/phontab").exists())
    }
}

val dataArchive = tasks.register<Zip>("createDataArchive") {
    isPreserveFileTimestamps = false
    isReproducibleFileOrder = true
    archiveFileName.set("espeakdata.zip")
    destinationDirectory.set(file("src/main/res/raw"))

    from(layout.buildDirectory.dir("generated/espeak-ng-data"))
    into("espeak-ng-data")
}

val dataHash = tasks.register<Checksum>("createDataHash") {
    checksumAlgorithm.set(Checksum.Algorithm.SHA256)
    inputFiles.setFrom(dataArchive)
    outputDirectory.set(layout.buildDirectory.dir("intermediates/datahash"))
}

tasks.register<Copy>("createDataVersion") {
    from(dataHash)
    rename("espeakdata.zip.sha256", "espeakdata_version")
    into(file("./src/main/res/raw"))
}

project.afterEvaluate {
    tasks.named("checkData") { dependsOn("externalNativeBuildDebug") }
    tasks.named("createDataArchive") { dependsOn("externalNativeBuildDebug") }
    tasks.named("javaPreCompileDebug") { dependsOn("createDataVersion") }
    tasks.named("javaPreCompileRelease") { dependsOn("createDataVersion") }
}
