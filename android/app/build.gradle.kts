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

        testInstrumentationRunner = "android.test.InstrumentationTestRunner"
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
    }
}

dependencies {
    implementation("androidx.appcompat:appcompat:1.6.1")
    androidTestImplementation("com.google.android:android-test:4.1.1.4")
    androidTestImplementation("org.hamcrest:hamcrest-library:1.3")
}

tasks.register("checkData") {
    doFirst {
        val dataFiles = listOf(
            "en_dict",
            "intonations",
            "phondata",
            "phondata-manifest",
            "phonindex",
            "phontab"
        )
        dataFiles.forEach {
            assert(file("../../espeak-ng-data/$it").exists()) {
                "Data file $it not found."
            }
        }
    }
}

val dataArchive by tasks.register<Zip>("createDataArchive") {
    isPreserveFileTimestamps = false
    isReproducibleFileOrder = true
    archiveFileName.set("espeakdata.zip")
    destinationDirectory.set(file("src/main/res/raw"))

    from(layout.buildDirectory.dir("generated/espeak-ng-data"))
    into("espeak-ng-data")
}

val dataHash by tasks.register<Checksum>("createDataHash") {
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
    tasks.named("checkData") {
        dependsOn("externalNativeBuildDebug")
    }

    tasks.named("createDataArchive") {
        dependsOn("checkData")
    }

    val tasksDependingOnCreateDataVersion = listOf(
        "mapDebugSourceSetPaths",
        "mapReleaseSourceSetPaths",
        "mergeDebugResources",
        "mergeReleaseResources",
        "packageDebugResources",
        "packageReleaseResources",
        "lintVitalAnalyzeRelease"
    )

    tasksDependingOnCreateDataVersion.forEach { taskName ->
        tasks.named(taskName) {
            dependsOn("createDataVersion")
        }
    }
}
