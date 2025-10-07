import math

# Configuration
element_count = 1000
z_min = -5.0
z_max = 5.0
step = (z_max - z_min) / (element_count - 1)

# Gaussian PDF function (standard normal distribution)
def gaussian_pdf(z):
    return (1.0 / math.sqrt(2 * math.pi)) * math.exp(-0.5 * z * z)

# Generate table
values = [gaussian_pdf(z_min + i * step) for i in range(element_count)]

# Format as C array
print("float gaussian_pdf_lookup(float z){")
print(f"    const int elmnt_count = {element_count};")
print("    const float results[elmnt_count] = {")

# Print 5 values per line for readability
for i in range(0, element_count, 5):
    line = ", ".join(f"{v:.8f}" for v in values[i:i+5])
    print(f"        {line},")

print("    };")
print("    if (z < -5.0f || z > 5.0f) return 0.0f;")
print(f"    int index = (int)((z + 5.0f) / {step:.8f});")
print("    return results[index];")
print("}")
