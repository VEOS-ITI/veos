# VEOS AIDL Interface Guide

## 1. Versioning of AIDL Interface

To update and freeze the AIDL interface version:

```bash
make android.vendor.test-update-api
make android.vendor.test-freeze-api
```

---

## 2. Build NDK AIDL Interface

```bash
make android.vendor.test-V1-ndk
```

---

## 3. Build Java AIDL Interface

```bash
make android.vendor.test-V1-java
```

---

## 4. Check the Location of Generated AIDL Outputs

```bash
cd out/soong/.intermediates/vendor/Aidl_Service/interfaces/aidl/android.vendor.test/android.vendor.test-api/
```

---

