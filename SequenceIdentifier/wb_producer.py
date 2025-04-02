from PIL import Image

def enlarge_image(image, scale_factor):
    width, height = image.size
    new_size = (int(width * scale_factor), int(height * scale_factor))
    return image.resize(new_size, Image.Resampling.BICUBIC)

blackboard = Image.new("1", (1920, 1080), 0)
blackboard.save("./white_blocks/0.bmp")
white_block = Image.new("1", (10, 10), 255)
for scale_factor in range(1, 101):
    to_be_pasted = enlarge_image(white_block, scale_factor).rotate(45, expand=True)
    blackboard_width, blackboard_height = blackboard.size
    image_width, image_height = to_be_pasted.size
    position = ((blackboard_width - image_width) // 2, (blackboard_height - image_height) // 2)
    blackboard.paste(to_be_pasted, position)
    blackboard.save(f"./white_blocks/{scale_factor}.bmp")
    