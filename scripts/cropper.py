from PIL import Image, ImageDraw, ImageFilter

def round_corners(im, rad):
    circle = Image.new('L', (rad * 2, rad * 2), 0)
    draw = ImageDraw.Draw(circle)
    draw.ellipse((0, 0, rad * 2 - 1, rad * 2 - 1), fill=255)
    alpha = Image.new('L', im.size, 255)
    w, h = im.size
    alpha.paste(circle.crop((0, 0, rad, rad)), (0, 0))
    alpha.paste(circle.crop((0, rad, rad, rad * 2)), (0, h - rad))
    alpha.paste(circle.crop((rad, 0, rad * 2, rad)), (w - rad, 0))
    alpha.paste(circle.crop((rad, rad, rad * 2, rad * 2)), (w - rad, h - rad))
    blurred_alpha = alpha.filter(ImageFilter.GaussianBlur(radius = 0.5))
    im.putalpha(blurred_alpha)
    return im.convert('RGBA')

def do_crop(path, N, M, names, radius):
    assert(len(names) == N)

    for i in range(N):
        assert(len(names[i]) == M)

    img = Image.open(path)
    w = img.size[0] / M
    h = img.size[1] / N

    assert(w == int(w))
    assert(h == int(h))

    def to_img_name(name):
        name = name.replace(' ', '_')
        return f'imgs/{name}.png'

    for i in range(N):
        for j in range(M):
            name = names[i][j]
            if name == 'none':
                continue
            x = j * w
            y = i * h
            reg = (x, y, x + w, y + h)
            obj = round_corners(img.crop(reg), radius)
            obj.save(to_img_name(name))
            if 'deck' in name:
                name_rot = f'{name} rotated'
                obj_rot = obj.transpose(Image.Transpose.ROTATE_90)
                obj_rot.save(to_img_name(name_rot))

def cards():
    path = 'orig/buildings.jpg'
    N = 8
    M = 12

    names = [
        ['lumber yard',
        'stone pit',
        'clay pool',
        'logging camp',
        'quarry',
        'clay pit',
        'glassworks',
        'press',
        'stable',
        'garrison',
        'palisade',
        'guard tower'],
        ['scriptorium',
        'workshop',
        'pharmacist',
        'apothecary',
        'tavern',
        'stone reserve',
        'clay reserve',
        'wood reserve',
        'theater',
        'altar',
        'baths',
        'sawmill'],
        ['shelf quarry',
        'brickyard',
        'glassblower',
        'drying room',
        'horse breeders',
        'barracks',
        'walls',
        'archery range',
        'parade ground',
        'school',
        'laboratory',
        'dispensary'],
        ['library',
        'brewery',
        'forum',
        'caravansery',
        'customs house',
        'temple',
        'statue',
        'tribunal',
        'aqueduct',
        'rostrum',
        'circus',
        'arsenal'],
        ['siege workshop',
        'fortifications',
        'courthouse',
        'academy',
        'university',
        'study',
        'observatory',
        'arena',
        'chamber of commerce',
        'port',
        'lighthouse',
        'armory'],
        ['palace',
        'gardens',
        'pantheon',
        'town hall',
        'senate',
        'obelisk',
        'merchants guild',
        'shipowners guild',
        'builders guild',
        'magistrates guild',
        'scientists guild',
        'moneylenders guild'],
        ['tacticians guild',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'deck age 1',
        'deck age 2'],
        ['deck age 3',
        'deck guilds',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none',
        'none']
    ]

    do_crop(path, N, M, names, 14)

def wonders():
    path = 'orig/wonders.jpg'
    N = 4
    M = 5

    names = [
        ['the pyramids',
        'the colossus',
        'the great lighthouse',
        'the temple of artemis',
        'the mausoleum'],
        ['the great library',
        'pireaus',
        'the hanging gardens',
        'the statue of zeus',
        'the sphinx'],
        ['the appian way',
        'circus maximus',
        'none',
        'none',
        'none'],
        ['none',
        'deck wonders',
        'none',
        'none',
        'none']
    ]

    do_crop(path, N, M, names, 18)

def tokens():
    path = 'orig/progress_tokens.jpg'
    N = 4
    M = 4

    names = [
        ['agriculture',
        'architecture',
        'economy',
        'law'],
        ['masonry',
        'mathematics',
        'philosophy',
        'strategy'],
        ['theology',
        'urbanism',
        'none',
        'none'],
        ['none',
        'none',
        'none',
        'deck tokens']
    ]

    do_crop(path, N, M, names, 100)

cards()
wonders()
tokens()
