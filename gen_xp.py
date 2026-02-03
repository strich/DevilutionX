
import os

def generate_curve():
    max_level = 99
    max_xp = 4000000000
    base_xp = 2000 # Level 1
    
    # Formula: XP = A * (Level)^B
    # 2000 = A * 1^B  -> A = 2000
    # 4000000000 = 2000 * 99^B
    # 2000000 = 99^B
    # B = ln(2000000) / ln(99)
    
    import math
    A = 2000
    B = math.log(2000000) / math.log(99)
    
    output_path = os.path.join("assets", "txtdata", "Experience.tsv")
    
    with open(output_path, "w", encoding="utf-8") as f:
        f.write("Level\tExperience\n")
        for lvl in range(1, max_level + 1):
            xp = int(A * (lvl ** B))
            # Ensure strict monotonicity and exact endpoint
            if lvl == max_level:
                xp = max_xp
            elif lvl == 1:
                xp = base_xp
                
            f.write(f"{lvl}\t{xp}\n")
            
    print(f"Generated {output_path}")

generate_curve()
